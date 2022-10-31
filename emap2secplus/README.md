# DeepMainMast




### 1. Make Atom Predictions by DeepMainMast
```
python3 main.py --mode=0 -F=[Map_Path] -M=[Model_Path] --contour=[half_contour_level] --gpu=[GPU_ID] --batch_size=[batch_size] --type=0
```
[Map_Path] is the path of the experimental cryo-EM map, [Model_Path] is the path of our pre-trained deep learning model, [half_contour_level] is 0.5* contour_level (suggested by author) to remove outside regions to save processing time, [GPU_ID] specifies the gpu used for inference, [batch_size] is the number of examples per batch in the inference (we used 16 with a 24GB GPU).

The predicted probability maps are saved in [Predict_Result/Protein_Detection/(map_name)/] with mrc format. It will include 7 mrc files corresponding to 7 different classes.

Example Command:
```
python3 main.py --mode=0 -F=example/8743.mrc -M=best_model/Unet_Protein_Atom.pth.tar --contour=0.0275 --gpu=0 --batch_size=16 --type=0
```

### 2.  Make Amino Acid Predictions by DeepMainMast

```
python3 main.py --mode=0 -F=[Map_Path] -M=[Model_Path] --contour=[half_contour_level] --gpu=[GPU_ID] --batch_size=[batch_size] --type=1
```
[Map_Path] is the path of the experimental cryo-EM map, [Model_Path] is the path of our pre-trained deep learning model, [half_contour_level] is 0.5* contour_level (suggested by author) to remove outside regions to save processing time, [GPU_ID] specifies the gpu used for inference, [batch_size] is the number of examples per batch in the inference (we used 16 with a 24GB GPU).

The predicted probability maps are saved in [Predict_Result/Protein_Detection/(map_name)/] with mrc format. It will include 20 mrc files corresponding to 20 different classes.

Example Command:
```
python3 main.py --mode=0 -F=example/8743.mrc -M=best_model/Unet_Protein_AA.pth.tar --contour=0.0275 --gpu=0 --batch_size=16 --type=1
```
