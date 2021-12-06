require("ifj21")
function program()
   local y = 20
   if 1 == 1 then
      y = 10
      write(y)
   else
      write(y)
   end
   write(y)
   local i = 1
   while i <= 10 do
      local i = i + 5
      i = i + 1
      write(i)

      write("Insert non-empty input for quiting, otherwise next iteration.\n")
      local inp = reads()
      if #inp ~= 0 then return else end
   end
end
program()
