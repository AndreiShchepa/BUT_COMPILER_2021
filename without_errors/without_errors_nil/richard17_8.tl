require "ifj21"


function main ()
	local a : string = nil
	local f : string = "string"
	if nil == f then
		local b : string = ((a).."aaa")..nil
		write(b)
	else end

end

main()
