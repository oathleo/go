package main

import "fmt"

func main() {

	s := make([]int, 10, 100)

	s1 := []int{1, 2, 3}
	s2 := []int{4, 5, 6, 7}
	fmt.Println(s1, s2)

	//  s1和s2指向同一个底层数组，copy只是数据上的变化，而没有影响到各个切片的指向位置！
	copy(s[1:4], s1)
	fmt.Println(s, s1, s2)

//	s1[0] = 9
//	fmt.Println(s, s1, s2)
}
