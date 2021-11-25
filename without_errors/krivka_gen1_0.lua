
require("ifj21")

function factorial(n)
   local n1 = n - 1
   if n < 2 then
      return 1
   else
      local tmp = factorial(n1)
      return n * tmp
   end
end

function main()
   write("Zadejte cislo pro vypocet faktorialu: ")
   local a = readi()
   if a ~= nil then
      if a < 0 then
         write("Faktorial nejde spocitat!", "\n")
      else
         local vysl = factorial(a)
         write("Vysledek je ", vysl, "\n")
      end
   else
      write("Chyba pri nacitani celeho cisla!\n")
   end
end

main()
