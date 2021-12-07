require("ifj21")



function main(num)
   local id = tointeger(num)
   local str_var
   local nil_var

   if str_var == nil_var then
      local len_str = 10
      str_var = func(id, len_str)
   else end

   write(str_var, "\x0A")
   return
end

function func(a, b)
   local id_str = ""
   local counter = 0

   while counter < b do
      id_str = id_str .. "A"
      counter = counter + 1
   end

   write(a, "\010")
   return id_str
end

main(10.9)
main(12.8e-2)
