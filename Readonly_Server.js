var position,len_v,num_value;
var myValue = new Array()

position = shared_val.indexOf("\n")
num_value = shared_val.substring(0,position)
shared_val = shared_val.substring(position+1)

for (var i = 0; i < num_value; i++) {
	position = shared_val.indexOf("\n")
	len_v = parseInt(shared_val.substring(0,position))
	myValue.push(shared_val.substring(position+1, position+1+len_v))
	shared_val = shared_val.substring(position+1+len_v+1)
}