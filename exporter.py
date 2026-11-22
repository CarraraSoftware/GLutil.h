from dataclasses import dataclass
import bpy

@dataclass
class Position:
    x: float
    y: float
    z: float

@dataclass
class Orientation:
    x: float
    y: float
    z: float
    w: float


@dataclass
class KeyFrame:
    scale: float
    position: Position
    orientation: Orientation


def main():
    objs = bpy.context.scene.objects
    out = None
    try:
        out = bpy.data.texts["out"] 
    except:
        out = bpy.data.texts.new("out")
    
    if not out:
        return
        
    
    if len(objs) < 1:
        out.write("No Objects Found.\n")    
        return
    
    cube = objs[0]
    # assume first object is the cube
    
    anim_data = cube.animation_data
    if not anim_data:
        out.write("Cube has no animation data.\n");
        return
        
    action = anim_data.action
    if not action:
        out.write("Cube has no action in its animation data.\n");
        return
       
    fcurves = action.fcurves
    
    keyframes = {}

    X_INDEX = 0
    Y_INDEX = 1
    Z_INDEX = 2
    W_INDEX = 3

    for fcurve in fcurves:
        if fcurve.data_path == "location":
            for kf in fcurve.keyframe_points:
                keyframe = keyframes.get(kf.co.x, None)
                if not keyframe:
                    keyframe = KeyFrame(1, Position(0, 0, 0), Orientation(0, 0, 0, 0))

                if fcurve.array_index == X_INDEX:
                    keyframe.position.x = kf.co.y
                if fcurve.array_index == Y_INDEX:
                    keyframe.position.y = kf.co.y
                if fcurve.array_index == Z_INDEX:
                    keyframe.position.z = kf.co.y

                keyframes[kf.co.x] = keyframe
            

        if fcurve.data_path == "rotation_quaternion":
            for kf in fcurve.keyframe_points:
                keyframe = keyframes.get(kf.co.x, None)
                if not keyframe:
                    keyframe = KeyFrame(1, Position(0, 0, 0), Orientation(0, 0, 0, 0))

                if fcurve.array_index == 0: # w (blender)
                    keyframe.orientation.w = kf.co.y
                if fcurve.array_index == 1: # x (blender)
                    keyframe.orientation.x = kf.co.y
                if fcurve.array_index == 2: # y (blender)
                    keyframe.orientation.z = kf.co.y
                if fcurve.array_index == 3: # z (blender)
                    keyframe.orientation.y = kf.co.y

                keyframes[kf.co.x] = keyframe

        if fcurve.data_path == "scale":
            for kf in fcurve.keyframe_points:
                keyframe = keyframes.get(kf.co.x, None)
                if not keyframe:
                    keyframe = KeyFrame(1, Position(0, 0, 0), Orientation(0, 0, 0, 0))

                if fcurve.array_index == X_INDEX:
                    keyframe.scale = kf.co.y
    
    

    name = os.path.dirname(bpy.data.filepath)
    f = open(f"{name}/cube.anim", "w")
    for key, value in keyframes:
        f.write(f"{key}\n")
        f.write(f"{value.scale}\n")
        f.write(f"{value.position.x} {value.position.y} {value.position.z}\n")
        f.write(f"{value.orientation.x} {value.orientation.y} {value.orientation.z} {value.orientation.w}\n")
        f.write("\n")
    f.close()

    
main()

