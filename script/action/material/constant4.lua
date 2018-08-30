-- create node constant4

require "moon.mat"

local node = moon.mat.new_node("mat_constant4")
if node then
	node:set_pos(moon.mouse.get_position())
end