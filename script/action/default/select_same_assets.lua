-- select same asset with curr selection

local all_nodes = moon.scene.get_all_nodes()
local selection = moon.scene.get_selection()
local new_selection = {}
for i = 1, #selection do
	for j = 1, #all_nodes do
		if selection[i]:get_filepath() == all_nodes[j]:get_filepath() then
			table.insert(new_selection, all_nodes[j])
		end
	end
end
print(#new_selection)
moon.scene.set_selection(new_selection)