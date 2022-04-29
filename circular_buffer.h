#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

template <class T, int buffer_size = 6000>
class circular_buffer{
  T buf[buffer_size];
  int head, tail, count;
  public:
  circular_buffer(): head{0}, tail{0}, count{0} {};
  
  bool is_empty() const {return count == 0;}
  bool is_full() const {return count == buffer_size;}
  int get_tail() const {return tail;}

  const T & operator[](int index) const {return buf[index];} //read index element
  
  void put(T elem){ //add element to buffer
    if(!is_full()){
      buf[head++] = elem;
      if(head == buffer_size) head = 0;
      count++;
    }
    //does nothing if buffer full
  }
  
  const T & take(){ //read and remove element from buffer
    T & tmp = buf[tail];
    if(!is_empty()){
      tail++; count--;
      if(tail == buffer_size) tail = 0;
    }
    return tmp;
  }
};

#endif
