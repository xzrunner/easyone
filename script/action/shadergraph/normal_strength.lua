-- create node NormalStrength

require "moon.sg"

local node = moon.sg.new_node("sg_normal_strength")
if node then
	node:set_pos(moon.mouse.get_position())
end