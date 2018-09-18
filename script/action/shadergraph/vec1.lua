-- create node Vector1

require "moon.sg"

local node = moon.sg.new_node("sg_vec1")
if node then
	node:set_pos(moon.mouse.get_position())
end