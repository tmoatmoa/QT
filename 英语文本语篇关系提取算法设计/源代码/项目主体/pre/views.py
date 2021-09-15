from django.shortcuts import render
from django import forms
import numpy as np
from django.http import FileResponse
import pickle
import gensim
import logging

logging.basicConfig(level=logging.CRITICAL)
import spacy
from keras.models import load_model
from rst import create_tree

from spacy.attrs import POS
class UploadFileForm(forms.Form):
    file = forms.FileField(label='上传待RST分析文本')

def download(request):
    file=open('result.txt','rb')
    response =FileResponse(file)
    response['Content-Type']='application/octet-stream'
    response['Content-Disposition']='attachment;filename="result.txt"'
    return response

def pre(sentence_list):
    print('预测中......')
    nlp = spacy.load("en")
    with open('relation_dicts.pickle', 'rb') as handle:
        relation2idx, idx2relation = pickle.load(handle)

    with open('pos2idx.pickle', 'rb') as handle:
        pos2idx = pickle.load(handle)

    EMBED_SIZE = 100
    embed_model = gensim.models.Word2Vec.load("word2vec")
    connection_model = load_model("connection_model.h5")
    relation_model = load_model("relation_model.h5")
    nuclearity_model = load_model("nuclearity_model.h5")

    def get_sentence_embedding(sentence, embed_model):
        embeddings = [embed_model[word] for word in sentence if word in embed_model.wv.vocab]
        if len(embeddings) == 0:
            return None
        word_sum = np.zeros(EMBED_SIZE, dtype='float64')
        word_count = 0
        for word in embeddings:
            word_sum += word
            word_count += 1
        return word_sum / word_count

    def get_sentence_vector(sentence, embed_model):
        embedding = get_sentence_embedding(sentence, embed_model)
        if embedding is None:
            return None
        doc = nlp(" ".join(sentence))
        root = [token for token in doc if token.head == token][0]
        return np.r_[len(sentence), (np.arange(POS) == pos2idx[root.pos_]).astype(np.float64), embedding]

    def get_vector(lhs, rhs, embed_model):
        if lhs.text is None or rhs.text is None:
            return None
        lhs_vector = get_sentence_vector(lhs.text, embed_model)
        if lhs_vector is None:
            return None
        rhs_vector = get_sentence_vector(rhs.text, embed_model)
        if rhs_vector is None:
            return None
        return np.r_[lhs_vector, rhs_vector]

    result = create_tree(
        sentence_list,
        lambda lhs, rhs: get_vector(lhs, rhs, embed_model),
        idx2relation,
        connection_model,
        relation_model,
        nuclearity_model)
    file=open('result.txt','w+',encoding='utf-8')
    file.write(result.output_lisp())
    file.close()
    print('预测结束！')
    return result.output_lisp()


def handle_upload_file(file):
    with open('test.txt', 'wb+') as destination:
        for chunk in file.chunks():
            destination.write(chunk)





def MyClassify(request):
    context={}
    if request.method == 'POST':
        form = UploadFileForm(request.POST, request.FILES)

        if form.is_valid():
            handle_upload_file(request.FILES.get('file', None))
            ftemp = request.FILES.get('file', None)
            print('ftemp: ', ftemp)

            texts = open('test.txt', encoding='utf-8').readlines()
            sents = []
            for text in texts:
                sents.append(text.strip())
            print(sents)
            result=pre(sents)
            results=result.split('\n')
            print(results)
            context['results']=result
            context['yuanwen']=sents

            print(context['results'])
            return render(request, 'result.html', context)
    else:
        form = UploadFileForm()

    return render(request, 'suanfa.html', {'form':form})






def result(request):

    result = u'成功......'
    return render(request, 'result.html', {'result':result})