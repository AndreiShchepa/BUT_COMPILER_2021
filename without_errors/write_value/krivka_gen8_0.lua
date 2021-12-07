require("ifj21")

function foo(x, y)
   local i = x
   local j = (y + 2) * 3
   i, j = j + 1, i + 1
   return i, j
end

function main()
   local a = 1
   local b = 2
   a, b = foo(a, b)
   if a < b then
      write(a, "<", b, "\n")
   else
      write(a, ">=", b, "\n")
      local a = 666
      write(a, "\n")
   end
   write(a, "\n")
end
main()
