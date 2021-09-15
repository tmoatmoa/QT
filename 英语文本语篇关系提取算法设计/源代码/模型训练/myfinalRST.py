#!/usr/bin/env python
# coding: utf-8

# In[1]:


from rst import RSTTree


# ###  加载词典

# In[2]:


import pickle

with open('mydata/relation_dicts.pickle', 'rb') as handle:
    relation2idx, idx2relation = pickle.load(handle)

with open('mydata/pos2idx.pickle', 'rb') as handle:
    pos2idx = pickle.load(handle)


# In[4]:


EMBED_SIZE = 100


# ### 加载训练测试数据

# In[5]:


with open('mydata/connection_train_set.pickle', 'rb') as handle:
    (conn_train_X, conn_train_Y) = pickle.load(handle)

with open('mydata/connection_test_set.pickle', 'rb') as handle:
    (conn_test_X, conn_test_Y) = pickle.load(handle)


# In[6]:


with open('mydata/relation_train_set.pickle', 'rb') as handle:
    (rel_train_X, rel_train_Y) = pickle.load(handle)

with open('mydata/relation_test_set.pickle', 'rb') as handle:
    (rel_test_X, rel_test_Y) = pickle.load(handle)


# In[7]:


with open('mydata/nuclearity_train_set.pickle', 'rb') as handle:
    (nuc_train_X, nuc_train_Y) = pickle.load(handle)

with open('mydata/nuclearity_test_set.pickle', 'rb') as handle:
    (nuc_test_X, nuc_test_Y) = pickle.load(handle)


# ###  训练connection classifier

# In[9]:


import keras
from keras import regularizers
from keras.models import Sequential
from keras.layers import Dense, Dropout


# In[10]:


connection_model = Sequential()
connection_model.add(Dense(256, input_dim=conn_train_X.shape[1], activation='relu'))
connection_model.add(Dropout(0.5))
connection_model.add(Dense(128, activation='relu'))
connection_model.add(Dropout(0.5))
connection_model.add(Dense(64, activation='relu'))
connection_model.add(Dropout(0.5))
connection_model.add(Dense(1, activation='sigmoid'))
connection_model.compile(loss='binary_crossentropy', optimizer="adam", metrics=['accuracy'])


# In[11]:


connection_model.fit(conn_train_X, conn_train_Y, batch_size=128, verbose=0, epochs=15)
connection_model.save("mymodel/connection_model.h5")


# ### 评估 模型

# In[12]:


score = connection_model.evaluate(conn_train_X, conn_train_Y, verbose=0)
print(score)


# In[13]:


score = connection_model.evaluate(conn_test_X, conn_test_Y, verbose=0)
print(score)


# ### 训练relation classifier

# In[14]:


relation_model = Sequential()
relation_model.add(Dense(256, input_dim=rel_train_X.shape[1], activation='relu'))
relation_model.add(Dropout(0.5))
relation_model.add(Dense(128, activation='relu'))
relation_model.add(Dropout(0.5))
relation_model.add(Dense(64, activation='relu'))
relation_model.add(Dropout(0.5))
relation_model.add(Dense(rel_train_Y.shape[1], activation='softmax'))
relation_model.compile(loss='categorical_crossentropy', optimizer="adam", metrics=['accuracy'])


# In[15]:


relation_model.fit(rel_train_X, rel_train_Y, batch_size=128, verbose=0, epochs=50)
relation_model.save("mymodel/relation_model.h5")


# ### 评估模型

# In[16]:


score = relation_model.evaluate(rel_train_X, rel_train_Y, verbose=0)
print(score)


# In[17]:


score = relation_model.evaluate(rel_test_X, rel_test_Y, verbose=0)
print(score)


# ## 训练 nuclearity classifier

# In[18]:


nuclearity_model = Sequential()
nuclearity_model.add(Dense(256, input_dim=nuc_train_X.shape[1], activation='relu'))
nuclearity_model.add(Dropout(0.5))
nuclearity_model.add(Dense(128, activation='relu'))
nuclearity_model.add(Dropout(0.5))
nuclearity_model.add(Dense(64, activation='relu'))
nuclearity_model.add(Dropout(0.5))
nuclearity_model.add(Dense(nuc_train_Y.shape[1], activation='softmax'))
nuclearity_model.compile(loss='categorical_crossentropy', optimizer="adam", metrics=['accuracy'])


# In[19]:


nuclearity_model.fit(nuc_train_X, nuc_train_Y, batch_size=128, verbose=0, epochs=10)
nuclearity_model.save("mymodel/nuclearity_model.h5")


# ### 评估模型

# In[20]:


score = nuclearity_model.evaluate(nuc_train_X, nuc_train_Y, verbose=0)
print(score)


# In[21]:


score = nuclearity_model.evaluate(nuc_test_X, nuc_test_Y, verbose=0)
print(score)


# In[23]:


### 模型预测见web前端


# In[ ]:




