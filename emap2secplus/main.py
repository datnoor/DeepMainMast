import os
from ops.argparser import argparser
from ops.os_operation import mkdir
import time
def init_save_path(origin_map_path):
    save_path = os.path.join(os.getcwd(), 'Predict_Result')
    mkdir(save_path)
    save_path = os.path.join(save_path, 'Protein_Detection')
    mkdir(save_path)
    map_name = os.path.split(origin_map_path)[1].replace(".mrc", "")
    map_name = map_name.replace(".map", "")
    map_name = map_name.replace("(","").replace(")","")
    save_path = os.path.join(save_path, map_name)
    mkdir(save_path)
    return save_path,map_name


if __name__ == "__main__":
    params = argparser()
    if params['mode']==0:
        gpu_id = params['gpu']
        if gpu_id is not None:
            os.environ["CUDA_VISIBLE_DEVICES"] = gpu_id
        cur_map_path = os.path.abspath(params['F'])
        model_path = os.path.abspath(params['M'])
        save_path,map_name = init_save_path(cur_map_path)
        from data_processing.Resize_Map import Resize_Map
        cur_map_path = Resize_Map(cur_map_path,os.path.join(save_path,map_name+".mrc"))
        from predict.detect_map import detect_map

        detect_map(cur_map_path,model_path,save_path,
                                  params['box_size'],params['stride'],params['batch_size'],params['contour'],params)



