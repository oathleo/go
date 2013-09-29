package main

import (
	"bytes"
	"compress/gzip"
	"fmt"
	"math/rand"
	"msgpack"
	"time"
)

type HisCollection struct {
	RTValues []RTValue
}

type RTValue struct {
	Time   int32
	Status int16
	Value  float32
}

func main() {
	size := 1000000
	fmt.Println("start:", size)

	col := make([]RTValue, size)

	for i := 0; i < size; i++ {
		col[i] = RTValue{Time: int32(i), Status: 100, Value: rand.Float32()}
	}

	his := HisCollection{RTValues: col}

	//	data, err := msgpack.Marshal(his)

	start := time.Now().UnixNano()
	data, err := msgpack.Marshal(&his)
	fmt.Println("Marshal cost:", (time.Now().UnixNano()-start)/1000000)

	if err != nil {
		panic(err)
	}
	fmt.Println("msgpack:", len(data))

	//un
	var unHis HisCollection
	//	msgpack.Unmarshal(data, &unHis)

	start = time.Now().UnixNano()
	msgpack.Unmarshal(data, &unHis)
	fmt.Println("Unmarshal cost:", (time.Now().UnixNano()-start)/1000000)

	fmt.Println("test data:" ,unHis.RTValues[1])


	var compress_data_buf bytes.Buffer
	writer := gzip.NewWriter(&compress_data_buf)
	defer writer.Close()

	writer.Write(data)
	writer.Flush()

	fmt.Println("msgpack gzip compress:", len(compress_data_buf.Bytes()))

	//	b, _ := msgpack.Marshal(true)
	//	fmt.Println("length:", len(b))
	//
	//	in := map[string]interface{}{"foo": 1, "hello": "world"}
	//	b2, _ := msgpack.Marshal(in)
	//	fmt.Println("length2:", len(b2))
}
