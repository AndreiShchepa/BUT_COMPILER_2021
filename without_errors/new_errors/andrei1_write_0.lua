require("ifj21")

function foo()
end

function main()
   local a = foo()

   if a ~= nil then
      write("NEFUNGUJE NAM TOTO\n\010\x0A\x0a")
   else
      write("POHODA\n\010\x0A\x0a")
   end
end

main()
