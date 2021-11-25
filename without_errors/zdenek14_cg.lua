require("ifj21")

function foo(a, b)
   return a, b
end

function main(a)
   local b
   a, b = foo(a, 3)
   write(a, b)
end

main(2)
