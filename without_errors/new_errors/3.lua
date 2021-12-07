require("ifj21")

function fn()
end

function main()
   local a = 1
   local b = 1
   local c = "hello"

   a, b, c = fn()

   if a == nil then
      write("yes\n")
   else
      write("no\n")
   end

   if b == nil then
      write("yes\n")
   else
      write("no\n")
   end

   if c == nil then
      write("yes\n")
   else
      write("no\n")
   end
end

main()
