-- create node DDX

require "moon.sg"

local node = moon.sg.new_node("sg_ddx")
if node then
	node:set_pos(moon.mouse.get_position())
end