package main

import (
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

	//	data := make([]byte, 0)
	//	buff := bytes.NewBuffer(data)
	var buff bytes.Buffer

	fmt.Println("buff init length:", buff.Len())
	var sendsize = 0
	start := time.Now().UnixNano()
	for i := 0; i < size; i++ {

		sendsize = sendsize + 10
		buff.Write(make([]byte, 10))

		//		leng, _ := buff.Write(WarpInt32ToByte(int32(col[i].Time)))
		//		sendsize = sendsize + leng
		//		leng, _ = buff.Write(WarpInt16ToByte(int16(col[i].Status)))
		//		sendsize = sendsize + leng
		//		leng, _ = buff.Write(WarpInt32ToByte(int32(col[i].Value)))
		//		sendsize = sendsize + leng
	}
	fmt.Println("send cost:", (time.Now().UnixNano()-start)/1000000, ":", sendsize)
	fmt.Println("buff:", buff.Len())

	start = time.Now().UnixNano()
	for i := 0; i < size; i++ {
	    buff.Read(make([]byte, 10))
//		temp := make([]byte, 4)
//		buff.Read(temp)
//		col[i].Time = WarpByteToInt32(temp)
//
//		temp = make([]byte, 2)
//		buff.Read(temp)
//		col[i].Status = WarpByteToInt16(temp)
//
//		temp = make([]byte, 4)
//		buff.Read(temp)
//		col[i].Value = WarpByteToFloat32(temp)
	}
	fmt.Println("recivice cost:", (time.Now().UnixNano()-start)/1000000)
	fmt.Println("recivice cost:",  buff.Len())

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

//切片转化为int32
func WarpByteToInt32(b []byte) int32 {
	//    return (((int) b[0]) << 24) + (((int) b[1]) << 16)
	//            + (((int) b[2]) << 8) + b[3];
	var value int32 = 0
	for i := 0; i < 4; i++ {
		shift := uint32((4 - 1 - i) * 8)
		value = value + (int32(b[i])&0x000000FF)<<shift
	}
	return value
}

func WarpByteToInt16(b []byte) int16 {
	var value int16 = 0
	for i := 0; i < 2; i++ {
		shift := uint16((2 - 1 - i) * 8)
		value = value + (int16(b[i])&0x000000FF)<<shift
	}
	return value
}

func WarpByteToFloat32(b []byte) float32 {
	return math.Float32frombits(uint32(WarpByteToInt32(b)))
}
