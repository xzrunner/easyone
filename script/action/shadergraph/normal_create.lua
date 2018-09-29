-- create node NormalCreate

require "moon.sg"

local node = moon.sg.new_node("sg_normal_create")
if node then
	node:set_pos(moon.mouse.get_position())
end