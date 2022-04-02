config = {
   filter="(" ..
	" proc.name=python3" ..
        ")" ..
	" and ( " ..
	" evt.type=sendto" ..
	" or evt.type=recvfrom" ..
	")"
}return config
