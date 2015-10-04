package main
import (
    "fmt"
    "net"
    "bufio"
    "os"
)

func main() {
    p :=  make([]byte, 2048)
    conn, err := net.Dial("udp", "127.0.0.1:8080")

    if err != nil {
        fmt.Printf("Some error %v", err)
        return
    }
    for i := 0; i < 3; i++ {
        reader := bufio.NewReader(os.Stdin)
        fmt.Print("Enter text: ")
        text, _ := reader.ReadString('\n')

        fmt.Fprintf(conn, text)

        _, err = bufio.NewReader(conn).Read(p)

        if err == nil {
            fmt.Printf("%s\n", p)
        } else {
            fmt.Printf("Some error %v\n", err)
        }
    }

    conn.Close()
}
