from django.urls import path
from . import views

app_name = 'pre'
urlpatterns = [
    path('result/', views.result, name='result'),
    path('classify/', views.MyClassify, name='classify'),
    path('download/', views.download, name='download'),

]