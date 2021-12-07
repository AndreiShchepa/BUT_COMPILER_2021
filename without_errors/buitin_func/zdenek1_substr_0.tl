require "ifj21"

function main()
	local str: string= "ahoj"
	local str_ret: string

	-- ok
	str_ret = substr(str, 1, 2)
	write(str_ret, "\n", "\n")
	
	str_ret = substr(str, 0, 3)
	write(str_ret, "\n", "\n")
	
	str_ret = substr(str, 0, 2)
	write(str_ret, "\n", "\n")
	
	str_ret = substr(str, 1, 3)
	write(str_ret, "\n", "\n")
	

	-- error
	local a:integer= 1-10
	local b:integer= 5-10

	str_ret = substr(str, a, b)
	write(str_ret, "\n", "\n")

	str_ret = substr(str, b, a)
	write(str_ret, "\n", "\n")
	
	str_ret = substr(str, a, 1)
	write(str_ret, "\n", "\n")

	str_ret = substr(str, 1, a)
	write(str_ret, "\n", "\n")
	
	str_ret = substr(str, 1, 7)
	write(str_ret, "\n", "\n")
	
	str_ret = substr(str, 7, 10)
	write(str_ret, "\n", "\n")

	str_ret = substr(str, 10, 7)
	write(str_ret, "\n", "\n")
	
end

main()
