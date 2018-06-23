package main

import (
	"fmt"
	"log"
	"net/http"
	"os"
)

func okHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintln(w, "OK")
}

func main() {
	port := os.Getenv("PORT")
	if port == "" {
		log.Fatal("$PORT must be set")
	}
	log.Printf("Listening at port: %s\n", port)
	http.HandleFunc("/", okHandler)
	log.Fatal(http.ListenAndServe(":"+port, nil))
}
