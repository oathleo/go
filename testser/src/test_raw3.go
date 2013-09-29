package main

import (
	"bufio"
	"bytes"
	"fmt"
	"math"
	"math/rand"
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
		col[i] = RTValue{Time: int32(i), Status: 100, Value: rand.Float32()}
	}

	b := bytes.NewBuffer(make([]byte, 1000000*10))
	bw := bufio.NewWriterSize(b, 1000000*10)
	fmt.Println(bw.Available()) // 4096
	fmt.Println(bw.Buffered())  // 0

	start := time.Now().UnixNano()
	for i := 0; i < size; i++ {
		bw.Write(WarpInt32ToByte(col[i].Time))
		bw.Write(WarpInt16ToByte(col[i].Status))
		bw.Write(WarpFloat32ToByte(col[i].Value))
	}
	fmt.Println("recivice cost:", (time.Now().UnixNano()-start)/1000000)
	
	fmt.Println(bw.Available()) // 4096
	fmt.Println(bw.Buffered())  // 0

}

//int32 转化为切片
func WarpInt32ToByte(num int32) []byte {
	return []byte{
		byte(num >> 24),
		byte(num >> 16),
		byte(num >> 8),
		byte(num)}
}

//int16 转化为切片
func WarpInt16ToByte(num int16) []byte {
	return []byte{
		byte(num >> 8),
		byte(num)}
}

//float32 转化为切片
func WarpFloat32ToByte(numfloat32 float32) []byte {
	var num uint32 = math.Float32bits(numfloat32)
	return []byte{
		byte(num >> 24),
		byte(num >> 16),
		byte(num >> 8),
		byte(num)}
}
