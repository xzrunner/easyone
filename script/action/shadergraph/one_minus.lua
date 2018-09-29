-- create node OneMinus

require "moon.sg"

local node = moon.sg.new_node("sg_one_minus")
if node then
	node:set_pos(moon.mouse.get_position())
end