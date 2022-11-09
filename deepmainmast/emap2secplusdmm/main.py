import os
from ops.argparser import argparser
from ops.os_operation import mkdir
import time

if __name__ == "__main__":
    params = argparser()
    if params['mode']==0:
        gpu_id = params['gpu']
        if gpu_id is not None:
            os.environ["CUDA_VISIBLE_DEVICES"] = gpu_id
        cur_map_path = os.path.abspath(params['F'])
        model_path = os.path.abspath(params['M'])
        save_path = f"output/{params['F'].split('/')[1]}/unet"
        map_name = params['F'].split('/')[1]
        from data_processing.Resize_Map import Resize_Map
        cur_map_path = Resize_Map(cur_map_path,os.path.join(save_path,map_name+".mrc"))
        from predict.detect_map import detect_map
        detect_map(cur_map_path,model_path,save_path,
                                  params['box_size'],params['stride'],params['batch_size'],params['contour'],params)



