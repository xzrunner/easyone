-- create node Vector2

require "moon.sg"

local node = moon.sg.new_node("sg_vec2")
if node then
	node:set_pos(moon.mouse.get_position())
end