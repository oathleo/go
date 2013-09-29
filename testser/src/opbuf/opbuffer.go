package opbuf

import (
	"errors"
	"fmt"
	"math"
)

type OPBuffer struct {
	data []byte
	off  int
}

func NewOPBuffer() *OPBuffer {
	return &OPBuffer{data: make([]byte, 1024), off: 0}
}

func NewOPBufferBySize(size int) *OPBuffer {
	return &OPBuffer{data: make([]byte, size), off: 0}
}

var EOF = errors.New("EOF")

func (b *OPBuffer) grow(length int) {
	capdata := cap(b.data)
	if b.off+length > capdata {
		//capdata 的倍数 2+n倍
		var newsize int = capdata * (length/capdata + 2)
		buf := make([]byte, newsize)
		copy(buf[:], b.data[0:b.off])
		b.data = buf
	}
}

func (b *OPBuffer) Flush() {
	b.off = 0
}

////////////////////////////int8
func (b *OPBuffer) GetInt8() (int8, error) {
	nextoff := b.off + 1
	if nextoff > len(b.data) {
		return 0, EOF
	}
	b.off = nextoff
	return int8(b.data[b.off-1]), nil
}

func (b *OPBuffer) PutInt8(num int8) {
	b.grow(1)
	b.data[b.off] = byte(num)
	b.off += 1
}

/////////////////////////////int16
func (b *OPBuffer) GetInt16() (int16, error) {
	nextoff := b.off + 2
	if nextoff > len(b.data) {
		return 0, EOF
	}
	b.off = nextoff
	return int16(b.data[b.off-2])<<8 | int16(b.data[b.off-1]), nil
}

func (b *OPBuffer) PutInt16(num int16) {
	b.grow(2)

	b.data[b.off] = byte(num >> 8)
	b.data[b.off+1] = byte(num)

	b.off += 2
}

////////////////////////////////int32
func (b *OPBuffer) GetInt32() (int32, error) {
	nextoff := b.off + 4
	if nextoff > len(b.data) {
		return 0, EOF
	}
	b.off = nextoff
	return int32(b.data[b.off-1]) | int32(b.data[b.off-2])<<8 | int32(b.data[b.off-3])<<16 | int32(b.data[b.off-4])<<24, nil
}

func (b *OPBuffer) PutInt32(num int32) {
	b.grow(4)

	b.data[b.off] = byte(num >> 24)
	b.data[b.off+1] = byte(num >> 16)
	b.data[b.off+2] = byte(num >> 8)
	b.data[b.off+3] = byte(num)

	b.off += 4
}

///////////////////////////////float32
func (b *OPBuffer) GetFloat32() (float32, error) {
	nextoff := b.off + 4
	if nextoff > len(b.data) {
		return 0, EOF
	}
	b.off = nextoff

	numuint32 := uint32(b.data[b.off-1]) | uint32(b.data[b.off-2])<<8 | uint32(b.data[b.off-3])<<16 | uint32(b.data[b.off-4])<<24
	return math.Float32frombits(numuint32), nil
}

func (b *OPBuffer) PutFloat32(numfloat32 float32) {
	b.grow(4)

	var num uint32 = math.Float32bits(numfloat32)
	b.data[b.off] = byte(num >> 24)
	b.data[b.off+1] = byte(num >> 16)
	b.data[b.off+2] = byte(num >> 8)
	b.data[b.off+3] = byte(num)

	b.off += 4
}

///////////////////////////////byte
//func (b *OPBuffer) GetByte(size int) ([]byte, error) {
//	nextoff := b.off + size
//	if nextoff > len(b.data) {
//		return nil, EOF
//	}
//	b.off = nextoff
//
//	numuint32 := uint32(b.data[b.off-1]) | uint32(b.data[b.off-2])<<8 | uint32(b.data[b.off-3])<<16 | uint32(b.data[b.off-4])<<24
//	return math.Float32frombits(numuint32)
//}

//func (b *OPBuffer) PutByte(d []byte) {
//	b.grow(len(d))
//	copy(b.data[b.off:b.off+len(d)], d)
//}
//
func (b *OPBuffer) Println() {
	fmt.Println("len:", len(b.data), ",off:", b.off)
}
