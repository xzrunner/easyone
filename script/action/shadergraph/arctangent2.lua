-- create node Arctangent2

require "moon.sg"

local node = moon.sg.new_node("sg_arctangent2")
if node then
	node:set_pos(moon.mouse.get_position())
end