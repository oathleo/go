package main

import (
	"fmt"
	"math/rand"
	"opbuf"
	"time"
)

type RTValue struct {
	Time   int32
	Status int16
	Value  float32
}

func main() {

	size := 1000000
	col := make([]RTValue, size)
	for i := 0; i < size; i++ {
		col[i] = RTValue{Time: int32(i), Status: int16(i), Value: rand.Float32()}
	}

	fmt.Println("send data:", col[size-1])
	var opbuff *opbuf.OPBuffer = opbuf.NewOPBuffer()
	start := time.Now().UnixNano()
	for i := 0; i < size; i++ {
		//		opbuff.PutByte(col[i].Data)
		opbuff.PutInt32(col[i].Time)
		opbuff.PutInt16(col[i].Status)
		opbuff.PutFloat32(col[i].Value)
	}
	fmt.Println("send cost:", (time.Now().UnixNano()-start)/1000000)

	opbuff.Flush()

	start = time.Now().UnixNano()
	for i := 0; i < size; i++ {
		col[i].Time,_ = opbuff.GetInt32()
		col[i].Status,_ = opbuff.GetInt16()
		col[i].Value,_ = opbuff.GetFloat32()
	}
	fmt.Println("rev cost:", (time.Now().UnixNano()-start)/1000000)
	fmt.Println("rev data:", col[size-1])

}
