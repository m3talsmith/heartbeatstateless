package main

import (
	"flag"
	"fmt"
	"log"
	"net"

	pb "github.com/m3talsmith/heartbeatstateless/server/heartbeat"
	"google.golang.org/grpc"
)

var (
	port = flag.Int("port", 3000, "Server port")
)

func init() {
	flag.Parse()
}

func main() {
	conn, err := net.Listen("tcp", fmt.Sprintf(":%d", *port))
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}
	server := grpc.NewServer()
	pb.RegisterHeartbeatAPIServer(server, &pb.Service{})
	log.Printf("server listening at %v", conn.Addr())
	log.Fatal(server.Serve(conn))
}
