#include <iostream>
#include <v8.h>
/**
* print_message prints each of its args. The args are passed from JavaScript. 
* Note that args are not necessarily typed in JavaScript. So while we assume 
* that the types are String, Number, Number, Boolean (with first number
* actually integer), we can't enforce that.
*/
v8::Handle<v8::Value> print_message(const v8::Arguments &args) { 
	v8::Locker locker;
	v8::HandleScope scope;
  // Print the string
	if (args.Length() > 0) {
		v8::String::Utf8Value message(args[0]->ToString()); 
		std::cout << *message;
	}
  // Print the integer
	if (args.Length() > 1) {
		uint64_t val = args[1]->ToInteger()->Value();
		std::cout << ", " << val;
	}

// Print the float
	if (args.Length() > 2) {
		double val = args[2]->ToNumber()->Value();
		std::cout << ", " << val;
	}
  // Print the bool part
	if (args.Length() > 3) {
		bool val = args[3]->ToBoolean()->BooleanValue(); 
		std::cout << ", " << (val ? "true" : "false");
	}
// newline
	std::cout << std::endl;
// If we had any args, we should return "true", else return "undefined"
	if (args.Length()) {
// This is tricky: we want to close out the scope, but we want to leak a 
// JavaScript Boolean object, so that we can access it from the script. return scope.Close(v8::Boolean::New(true));
	}
	return v8::Undefined();
}

