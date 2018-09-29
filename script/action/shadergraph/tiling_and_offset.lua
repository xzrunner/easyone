-- create node TilingAndOffset

require "moon.sg"

local node = moon.sg.new_node("sg_tiling_and_offset")
if node then
	node:set_pos(moon.mouse.get_position())
end