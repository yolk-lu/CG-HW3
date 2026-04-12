import numpy as np
from diffusers import StableDiffusionControlNetPipeline, ControlNetModel
import torch
import cv2
import os
from PIL import Image

def load_binary_depth_map(filepath, flip_vertical=False):
    """
    Reads the custom binary float array format exported by our C++ toolkit.
    Converts it into a normalized ControlNet-ready RGB Depth Map image.
    return : PIL.Image
    """
    if not os.path.exists(filepath):
        raise FileNotFoundError(f"Cannot find {filepath}")

    with open(filepath, "rb") as f:
        # First 8 bytes are width and height (2 int32s)
        hdr = np.frombuffer(f.read(8), dtype=np.int32)
        width, height = hdr[0], hdr[1]
        
        # The rest is the float32 array
        depth_data = np.frombuffer(f.read(), dtype=np.float32)
        
    depth_image = depth_data.reshape((height, width))
    
    # Normalize depth map to 0-255 grayscale
    # Depth ranges from 0.0 (near) to 1.0 (far/background).
    # ControlNet Depth prefers closer objects to be brighter (closer to 255).
    # We invert it: pixel = 255 * (1.0 - z)
    depth_image = np.clip((1.0 - depth_image) * 255.0, 0, 255).astype(np.uint8)
    
    # Convert to 3-channel RGB image for pipeline
    img = cv2.cvtColor(depth_image, cv2.COLOR_GRAY2RGB)
    
    pil_img = Image.fromarray(img)
    
    # OpenGL glReadPixels is fetched from Bottom-Left origin, so it's upside down.
    if flip_vertical:
        pil_img = pil_img.transpose(Image.FLIP_TOP_BOTTOM)
        
    return pil_img

def run_diffusion():
    print("Loading Pretrained Models")

    # you can change the model to speed up
    controlnet = ControlNetModel.from_pretrained(
        "lllyasviel/sd-controlnet-depth", 
        torch_dtype=torch.float16
    )
    
    pipe = StableDiffusionControlNetPipeline.from_pretrained(
        "runwayml/stable-diffusion-v1-5", 
        controlnet=controlnet, 
        torch_dtype=torch.float16
    )
    pipe.to("cuda")

    # Core aesthetics prompt(you can change it to get different results)
    prompt = "A high quality intricate 3d render of abstract geometric shapes, crystal glass texture, professional studio lighting, cinematic, 8k resolution, highly detailed materials, soft shadows, octane render"
    negative_prompt = "low quality, blurry, flat, ugly, text, out of focus, distorted"
    
    # Determine execution root (in case user runs from inside build/ or src/)
    base_dir = "build/" if os.path.exists("build/software_depth.bin") else "./"

    # Process Software Rasterizer Output
    print("\nProcessing Software Rasterization Output...")
    try:
        sw_depth = load_binary_depth_map(os.path.join(base_dir, "software_depth.bin"))
        
        # Save intermediate depth map view for debugging and verifying
        sw_depth.save("debug_sw_depth_map.png") 
        
        result_sw = pipe(
            prompt, 
            negative_prompt=negative_prompt, 
            image=sw_depth,
            num_inference_steps=20,
        ).images[0]
        
        result_sw.save("software_shaded_result.png")
        print("Saved generating output to: software_shaded_result.png")
    except Exception as e:
        print(f"Failed processing software depth: {e}")

    # Process OpenGL Rasterizer Output
    print("\nProcessing OpenGL Hardware Output...")
    try:
        # Pass flip_vertical=True because OpenGL reads starting from bottom-left natively
        gl_depth = load_binary_depth_map(os.path.join(base_dir, "opengl_depth.bin"), flip_vertical=True)
        gl_depth.save("debug_gl_depth_map.png")
        
        result_gl = pipe(
            prompt, 
            negative_prompt=negative_prompt, 
            image=gl_depth,
            num_inference_steps=20,
        ).images[0]
        
        result_gl.save("opengl_shaded_result.png")
        print("Saved generating output to : opengl_shaded_result.png")
    except Exception as e:
        print(f"Failed processing OpenGL depth: {e}")
        
    print("\n End ")

if __name__ == "__main__":
    run_diffusion()
