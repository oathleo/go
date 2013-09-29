package main

import (
	"bytes"
	"compress/gzip"
	"fmt"
	"math/rand"
	"math"
)

func main() {
	size := 1000000

	var compress_data_buf bytes.Buffer
	
	writer := gzip.NewWriter(&compress_data_buf)
	defer writer.Close()

	for i := 0; i < size; i++ {
		writer.Write(WarpInt32ToByte(int32(i)))
		writer.Write(WarpInt16ToByte(int16(100)))
		writer.Write(WarpFloat32ToByte(rand.Float32()))
	}

	writer.Flush()

	fmt.Println("raw data:", 1000000)

	fmt.Println("raw data gzip compress:", len(compress_data_buf.Bytes()))

}


//网络字节序

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

 
 
