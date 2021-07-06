//Creates an item on the heap yet allows it to be managed by a deque

#pragma once

#include<iostream>
#include<string>
#include<deque>
#include<algorithm>
#include<thread>

class _heap_class
{
public:
	std::string value = "default";
	
	_heap_class(){}
	_heap_class(_heap_class&& other) noexcept;
	_heap_class operator=(const _heap_class& other);
	_heap_class(const _heap_class& other);
	
	static _heap_class& append(_heap_class& var);
	static _heap_class& create();
	//static _heap_class& create(std::string value);
	//static int load(std::string path);
	//static int load_as_thread(std::string name);
	/*Examples broken to modified external code*/


	static std::deque<_heap_class> instances;

	/// The Above was my old technique for storing variables contained within deques on the heap
	/// My new technique is simpler, constructor-less, and better supports threading (ptr instead of int)
	/// It also avoids calling the copy constructor on big heavy constructed objects
	/// It is about half the size in code:
	
	static _heap_class* alloc();
	static _heap_class* alloc(std::string filepath);
	static _heap_class* alloc_thread(std::string filepath);
private:
	static void load_thread(_heap_class* ptr, std::string filepath);

};

