package heartbeat

import (
	"context"
	"log"
	"time"

	"google.golang.org/protobuf/types/known/emptypb"
)

type StatusBeat int

const (
	StatusBeatOffline StatusBeat = iota
	StatusBeatOnline
)

func (s StatusBeat) String() string {
	switch int(s) {
	case 1:
		return "Online"
	default:
		return "Offline"
	}
}

type CurrentBeat struct {
	beat   *Beat
	Status StatusBeat
}

var (
	beats    = make(map[string]*CurrentBeat)
	beatChan = make(chan *Beat)
)

func init() {
	// Listen for beats
	go func() {
		for {
			beat := <-beatChan
			beats[beat.Id] = &CurrentBeat{beat: beat, Status: StatusBeatOnline}
		}
	}()

	// Healthcheck beats
	go func() {
		for {
			currentTime := time.Now()
			for id, beat := range beats {
				lastSeen := time.Unix(int64(beat.beat.Timestamp), 0)
				if beat.Status == StatusBeatOnline {
					diff := currentTime.Sub(lastSeen)
					if diff > (time.Second * 2) {
						beat.Status = StatusBeatOffline
					}
					if beat.Status == StatusBeatOffline {
						log.Printf("[OFFLINE] id=%s, last_seen=%s", id, lastSeen)
					}
				}
			}
			time.Sleep(time.Second * 1)
		}
	}()
}

type Service struct {
	UnimplementedHeartbeatAPIServer
}

func (s Service) Send(ctx context.Context, beat *Beat) (*emptypb.Empty, error) {
	log.Printf("[BEAT] id=%s, last_seen=%s", beat.Id, time.Unix(int64(beat.Timestamp), 0))
	beatChan <- beat
	return &emptypb.Empty{}, nil
}
