#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <v8.h>
/**
 * Print an error (v8 exception) to stdout
 * @param error the v8 exception object
 */
void print_error(v8::TryCatch *error) {
  // standard v8 setup
  v8::Locker lock;
  v8::HandleScope handle_scope;
  // extract the message
  v8::String::Utf8Value exception(error->Exception()); 
  v8::Handle<v8::Message> message = error->Message();
  // handle error with no message:
  if (message.IsEmpty()) {
    std::cout << *exception << std::endl;
    return;
  }

// Get the name of the file, and the line that produced the error
  v8::String::Utf8Value file_name(message->GetScriptResourceName()); 
  int line_number = message->GetLineNumber();
  v8::String::Utf8Value source(message->GetSourceLine());
// Print the error as best we can
  std::cout << *file_name << ":" << line_number << ": " << *exception << std::endl
  << *source << std::endl;
}

/**
* Execute some JavaScript code. Also give that code a name, so we can try * to run it again in the future, by name.
* @param code A v8 string holding the code
* @param name A name to associate with this code
* @param context The v8 context in which to run the code
*/
bool run_code(v8::Handle<v8::String> code, v8::Handle<v8::Value> name, const v8::Handle<v8::Context> &context) {
  	// Standard v8 setup
 v8::Locker lock;
 v8::HandleScope handle_scope;
  	// Make sure the code to run is not empty
 if (code->Length() == 0)
   return false;
  	// Switch to the provided execution context
 v8::Context::Scope context_scope(context);
	// Try to compile the script code
 v8::TryCatch try_catch; // this picks up errors 
 v8::Handle<v8::Script> script = v8::Script::Compile(code, name);


	// Error if script empty (probably compilation error)
 if (script.IsEmpty()) {
   print_error(&try_catch);
   return false;
 }
	// Run the script. If result empty, probably runtime error
 v8::Handle<v8::Value> result = script->Run();
 if (result.IsEmpty()) {
   print_error(&try_catch);
   return false;
 }
	// If there is a result, print it to the console
 if (!result->IsUndefined() && !result.IsEmpty()) {
   v8::String::Utf8Value str(result);
   // std::cout << *str << std::endl;
 }
 return true; 
}

/**
 * Execute a script inside of an execution context
 *
 * @param script The name of the script file to run
 * @param context The v8 context within which to run the script
 */
bool run_script(const std::string text, const v8::Handle<v8::Context> &context) {
  std::string name = "JS_file";
 // Need a locker and a handle scope
  v8::Locker lock;
  v8::HandleScope handle_scope;
 // Load the file, turn into a v8 string, fail if empty
  v8::Handle<v8::String> source = v8::String::New(text.c_str(), text.length()); 
  if (source.IsEmpty())
    return false;
// Try to execute the file contents, either compiler fails or return success
  if (!run_code(source, v8::String::New(name.c_str()), context)) 
    return false;
  return true;
  }





