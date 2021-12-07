require("ifj21")

function main(str)
   local len = #str
   local string_var = substr(str, 8, len)

   if string_var == "search" then
      write("all good, this function has substr: ", string_var, "\n")
   else
      write("error with function substr", " or ", "no")
   end

   return

end

main("Str_for_search")
