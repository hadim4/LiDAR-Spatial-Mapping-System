# Final Project Python code - Mohammad Hadi

import serial
import math
import numpy as np
import open3d as o3d

cloud = []
x = 0
angle_step = 0
x_inc = 1000
counter = 0
f = open("tof_radar.xyz", "w")

s = serial.Serial("COM4", 115200, timeout=30)
print("Opening: " + s.name)

s.reset_output_buffer()
s.reset_input_buffer()

input("Press Enter to start communication...")

for i in range(96):
    counter += 1
    angle_step += 11.25
    distance_S = s.readline().decode().strip()
    distance = float(distance_S)
  
    if angle_step == 371.25:
        print("\n NEXT SCAN\n")
        angle_step = 11.25
        x = x + x_inc
        counter = 1
    
    angle = math.radians(angle_step)
    y = distance * math.sin(angle)
    z = distance * math.cos(angle)
    print(counter,", angle: ", angle_step, "distance: ", distance, "|    ", "x=", x, "y= ", y, "z= ", z)
    f.write(f"{x} {y} {z}\n")
    cloud.append([x, y, z])  # Store points in memory for line connections

f.close()
s.close()
print("Closing: " + s.name)

pcd = o3d.geometry.PointCloud()
pcd.points = o3d.utility.Vector3dVector(np.array(cloud))


scan = 3  # 24 points / 8 p
num = 32  # 

# Create line connections
mesh = []
for k in range(scan):
    base = k * num
    for i in range(num):
        mesh.append([base + i, base + (i + 1) % num])
        if k < scan - 1:
            mesh.append([base + i, base + i + num])

line_set = o3d.geometry.LineSet(points=o3d.utility.Vector3dVector(np.array(cloud)),lines=o3d.utility.Vector2iVector(np.array(mesh))
)

print("\n3D print generated")
o3d.visualization.draw_geometries([pcd, line_set])
