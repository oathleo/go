package main

import (
	"bytes"
	"compress/gzip"
	"fmt"
	"labix.org/v2/mgo/bson"
	"math/rand"
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
	fmt.Println("start")

	size := 1000000
	col := make([]RTValue, size)

	for i := 0; i < size; i++ {
		col[i] = RTValue{Time: int32(i), Status: 100, Value: rand.Float32()}
	}

	his := HisCollection{RTValues: col}
	//	data, err := bson.Marshal(&his)

	start := time.Now().UnixNano()
	data, err := bson.Marshal(&his)
	fmt.Println("Marshal cost:", (time.Now().UnixNano()-start)/1000000)

	if err != nil {
		panic(err)
	}
	//	fmt.Println(data)
	fmt.Println("bson byte:", len(data))

	var unHis HisCollection
	
	start = time.Now().UnixNano()
	bson.Unmarshal(data, &unHis)
	fmt.Println("Unmarshal cost:", (time.Now().UnixNano()-start)/1000000)
	fmt.Println("test data:", unHis.RTValues[1])

	var compress_data_buf bytes.Buffer
	writer := gzip.NewWriter(&compress_data_buf)
	defer writer.Close()

	writer.Write(data)
	writer.Flush()

	fmt.Println("bson gzip compress:", len(compress_data_buf.Bytes()))

}
