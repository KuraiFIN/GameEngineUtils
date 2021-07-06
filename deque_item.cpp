//Creates an item on the heap yet allows it to be managed by a deque

#include "deque_item.h"

std::deque<_heap_class> _heap_class::instances = {};


//Default(not needed)

//Move
_heap_class::_heap_class(_heap_class&& other) noexcept : value(other.value) {
	std::cout << "Moving instance: " << value << std::endl;
	//other.protect = true;//Main reason to use Move!!
	//vertices = other.vertices;
}
//Assign: Necessary for Move:
_heap_class _heap_class::operator=(const _heap_class& var) {
	std::cout << "Assigning instance: " << value << std::endl;
	return var;
}
//Copy: Necessary for Assign
_heap_class::_heap_class(const _heap_class& other) : value(other.value) {
	std::cout << "Copying instance: " << value << std::endl;
	//vertices = other.vertices;
}


_heap_class& _heap_class::append(_heap_class& instance) {
	instances.emplace_back(std::move(instance));
	std::cout << "Added instance '" << instance.value << "' to instances[size:" << instances.size() << "]" << std::endl;
	//mesh_count_internal += 1;
	return instances[instances.size() - 1];
}

_heap_class& _heap_class::create() {
	_heap_class instance = _heap_class();
	instance.value = "Empty";
	return append(instance);
}
/*
My old method:

_heap_class& _heap_class::create(std::string path) {
	_heap_class instance = ImportFileObj::load(path);
	return append(instance);
}

int _heap_class::load(std::string path) {
	//mesh_count_internal += 1;
	_heap_class instance = ImportFileObj::load(path);
	_heap_class& a = append(instance);
	mesh_count_internal -= 1;
	return meshes.size() - 1;
}

int _heap_class::load_as_thread(std::string path) {
	std::thread t(load, path);
	t.detach();//or join()
	return mesh_count_internal - 1;
}
*/


//New method:

_heap_class* _heap_class::alloc() {
	int index = instances.size();
	std::cout << "!? Alloc Mesh: " << instances.size() << std::endl;
	//meshes.resize(index + 1);//cannot access private constructor
	//copy constructor is still called (unavoidable), but does not have to be declared
	auto generator = []() {return _heap_class(); };
	std::generate_n(std::back_inserter(instances), 1, generator);
	_heap_class* output = &instances.at(index);
	return output;
}
_heap_class* _heap_class::alloc(std::string filepath) {
	_heap_class* output = alloc();
	output->value = filepath;
	//ImportFileObj::load(output, filepath);
	return output;
}
void _heap_class::load_thread(_heap_class* ptr, std::string filepath) {
	//mesh_count_internal += 1;
	//ImportFileObj::load(ptr, filepath);
}
_heap_class* _heap_class::alloc_thread(std::string filepath) {
	_heap_class* output = alloc();
	std::thread t(load_thread, output, filepath);
	t.detach();
	return output;
}

