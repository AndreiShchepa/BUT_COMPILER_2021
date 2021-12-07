require("ifj21")

function foo()
   write("aaaa\n")
   local b
   local a = tointeger(b)
   write("aaaa\n")
   return a
end

function main()
   write("bbb\n")
   local a = foo()
   write("bbb\n")
   if a ~= nil then
      write("FIX FUNCTION READ WITH BAD INPUTIN 1010\x0A\x0a", a, "\n")
   else
      write("POHODA\n\010\x0A\x0a", a, "\n")
   end
end

main()
