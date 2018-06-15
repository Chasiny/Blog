package message_queue

import (
	"testing"
	"math/rand"
	"fmt"
	"sync"
)

func TestNewMQ(t *testing.T) {
	mq:=NewMQ(2)

	m:=sync.Map{}

	tSize:=10000
	wg:=sync.WaitGroup{}
	wg.Add(tSize*2)

	go func() {
		for i:=0;i<tSize;i++{
			go func(i int) {
				in:=rand.Int()
				fmt.Println(i,"put",in)
				m.Store(in,true)
				mq.Put(in)
				fmt.Println(i,"put",in,"finish")
				wg.Done()
			}(i)
		}
	}()
	go func() {
		for i:=0;i<tSize;i++{
			go func(i int) {
				out:=mq.Get()
				fmt.Println(i,"get",out)
				m.Store(out,false)
				wg.Done()
			}(i)
		}
	}()

	wg.Wait()

	count:=0
	m.Range(func(key, value interface{}) bool {
		t.Log(key,value)
		if value.(bool){
			t.Fail()
		}
		count++
		return true
	})

	t.Log(count)
}

func TestNewMQ2(t *testing.T) {
	mq:=NewMQ(2)
	mq.Put(1)
	mq.Put(2)
	t.Log(mq.Get())
	t.Log(mq.Get())
	mq.Put(1)
	mq.Put(2)
	t.Log(mq.Get())
	t.Log(mq.Get())
	//mq.Put(3)
}