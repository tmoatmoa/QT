import re
import math
import numpy as np
  
def parse_sexp(sexp):
    """
    Transform lisp s-expression into nested list structure.
    All lisp features except for parentheses are ignored.
    Punctuation (. ,) are separated from the words.

    Args:
        sexp: s-expression to transform.

    Returns:
        Nested lists representing s-expression.
    """        

    term_regex = r"""(?mx)
        \s*(?:
            (?P<lparen>\()|
            (?P<rparen>\))|
            (?P<s>(\w|\-)+)|
            (?P<p>[\.,]+)
           )"""
    stack = []
    out = []
    for termtypes in re.finditer(term_regex, sexp):
        term, value = [(t,v) for t,v in termtypes.groupdict().items() if v][0]
        if term == 'lparen':
            stack.append(out)
            out = []
        elif term == 'rparen':
            assert stack, "Trouble with nesting of parentheses"
            tmpout, out = out, stack.pop(-1)
            out.append(tmpout)
        elif term == 's' or term == 'p':
            out.append(value.lower())
        else:
            raise NotImplementedError("Error: %r" % (term, value))
    assert not stack, "Trouble with nesting of parentheses"
    return out[0]

import itertools

class RSTTree:
    
    MONONUCLEAR = 0
    MULTINUCLEAR = 1
    
    def __init__(self):
        self.type = 'root'
        self.span = None
        self.text = None
        self.nuclei = []
        self.satellite = None
        self.is_leaf = False
        self.nuclearity = None
        self.index = -1
        self.relation = None        
        self.text_constructed = False
        self.spans_calculated = False
        self.flatten = lambda l: [item for sublist in l for item in sublist]
    
    @staticmethod
    def from_sexp(sexp, filename):
        """
        Initialize RST tree from s-expression.

        Args:
            sexp: s-expression representing RST tree.
            filename: name of the file corresponding to the lisp trees.

        Returns:
            A new RST tree.
        """
        list_tree = parse_sexp(sexp)
        return RSTTree.from_list(list_tree, filename)

    @staticmethod
    def from_list(list_tree, filename, index = -1, parent = None):
        """
        Initialize RST tree from s-expression.

        Args:
            list_tree: nested lists representing RST tree.
            filename: name of the file corresponding to the lisp trees.
            index: index of the nucleus un multinuclear relations.
            parent: parent of this tree.

        Returns:
            A new RST tree.
        """

        new_tree = RSTTree()
        new_tree.parent = parent
        new_tree.filename = filename
        new_tree.nuclearity = RSTTree.MULTINUCLEAR
        new_tree.index = index
        
        nucleus_index = 0
        new_tree.type = list_tree[0]
        for child in list_tree[1:]:
            child_tag = child[0]
            
            if child_tag == 'span':
                new_tree.span = int(child[1]), int(child[2])
                new_tree.spans_calculated = True
            
            elif child_tag == 'leaf':
                new_tree.is_leaf = True
                new_tree.span = int(child[1]), int(child[1])
                new_tree.spans_calculated = True
            
            elif child_tag == 'rel2par' and parent is not None and child[1] != "span":
                if parent.relation is None or parent.relation == "span":
                    parent.relation = child[1]

            elif child_tag == 'nucleus':
                new_tree.nuclei.append(RSTTree.from_list(child, filename, parent=new_tree, index=nucleus_index))
                nucleus_index += 1
            
            elif child_tag == 'satellite':
                new_tree.satellite = RSTTree.from_list(child, filename, parent=new_tree)
                new_tree.nuclearity = RSTTree.MONONUCLEAR
            
            elif child_tag == 'text':
                new_tree.text = []
                for item in child[1:]:
                    if isinstance(item, list):
                        new_tree.text += item
                    else:
                        new_tree.text.append(item)

        return new_tree

    @staticmethod
    def from_text(text, start, end):
        """
        Initialize RST tree from text.

        Args:
            text: as a list of tokens.
            start: span starting index.
            end: span ending index.

        Returns:
            A new RST tree.
        """
        new_tree = RSTTree()
        new_tree.text = text
        new_tree.span = (start, end)
        new_tree.is_leaf = True
        new_tree.text_constructed = True
        return new_tree

    @staticmethod
    def from_trees(relation, nuclei, satellite=None):
        """
        Initialize RST tree from child trees.

        Args:
            relation: relation type for this tree.
            nuclei: list of child nuclei RST trees.
            staellite: child satellite RST tree.

        Returns:
            A new RST tree.
        """        
        new_tree = RSTTree()
        new_tree.is_leaf = False
        new_tree.nuclei = nuclei
        new_tree.satellite = satellite
        new_tree.relation = relation
        new_tree.nuclearity = RSTTree.MULTINUCLEAR if satellite is None else RSTTree.MONONUCLEAR
        new_tree.calculate_spans()
        
        for nucleus in new_tree.nuclei:
            nucleus.type = 'nucleus'
            nucleus.parent = new_tree
        
        if new_tree.satellite is not None:
            new_tree.satellite.type = 'satellite'
            new_tree.satellite.parent = new_tree
        
        return new_tree

    def output_lisp(self, indent = 0):
        """
        Output the tree in the s-expression format.

        Args:
            indent: indentation for the output.

        Returns:
            A pretty string containing s-expression.
        """        

        rep = (" " * indent) + "( " + self.type + " "
        # 1. leaf or span
        if self.is_leaf:
            rep += "(leaf " + str(self.span[0]) + ") "
        else:
            rep += "(span " + str(self.span[0]) + " " + str(self.span[1]) + ") "
        # 2. rel2par
        if self.type == "nucleus":
            if self.parent.nuclearity == RSTTree.MULTINUCLEAR:
                rep += "(rel2par " + self.parent.relation + ") "
            else:
                rep += "(rel2par span) "
        elif self.type == "satellite":
            rep += "(rel2par " + self.parent.relation + ") "
        # 3. children
        if self.is_leaf:
            rep += "(text " + " ".join(self.text) + ") )"
        else:
            for nucleus in self.nuclei:
                rep += (" " * indent) + "\n" + nucleus.output_lisp(indent + 2)
            if self.satellite is not None:
                rep += (" " * indent) + "\n" + self.satellite.output_lisp(indent + 2)
            rep += "\n" + (" " * indent) + ")"
        return rep
    
    def calculate_spans(self):
        """
        Recursively calculate span for this tree from children trees.

        Returns:
            Current tree span.
        """ 
        if self.span is None:
            start = math.inf
            end = 0
            if self.nuclei is not None:
                start = min([nucleus.calculate_spans()[0] for nucleus in self.nuclei])
                end = max([nucleus.calculate_spans()[1] for nucleus in self.nuclei])
            if self.satellite is not None:
                start = min(start, self.satellite.calculate_spans()[0])
                end = max(end, self.satellite.calculate_spans()[1])
            self.span = (start, end)
        self.spans_calculated = True
        return self.span
        
    def construct_text(self):
        """
        Recursively construct text for this tree from children trees.
        The text is constructed by concatenating text retrieved from
        children.

        Returns:
            Current tree text.
        """
        assert self.spans_calculated, "Spans are not calculated: " + self.filename + " " + str(self.span)
        self.text_constructed = True
        if self.text is None:
            texts = []
            start = 0
            end = 0
            if self.nuclei is not None:
                texts = [nucleus.construct_text() for nucleus in self.nuclei]
                start = min([nucleus.calculate_spans()[0] for nucleus in self.nuclei])
                end = max([nucleus.calculate_spans()[1] for nucleus in self.nuclei])
            if self.satellite is not None:
                start_satellite = self.satellite.calculate_spans()[0]
                satellite_text = self.satellite.construct_text()
                if start_satellite > end:
                    texts.append(satellite_text)
                else:
                    texts = [satellite_text] + texts
            self.text = list(itertools.chain.from_iterable(texts))
        return self.text

    def get_subtexts(self):
        """
        Retrieve a list of texts from all children of this tree.

        Returns:
            A list of texts from children.
        """
        assert self.text_constructed, "Texts are not constructed"
        subtexts = []
        if not self.is_leaf:
            if self.nuclei is not None:
                subtexts += [nucleus.text for nucleus in self.nuclei]
            if self.satellite is not None:
                subtexts.append(self.satellite.text)
        return subtexts
    
    def all_trees(self):
        """
        Retrieve a list of subtrees of this tree, including itself but excluding
        all the leafs.

        Returns:
            A list subtrees.
        """
        subtrees = []
        if self.nuclei is not None:
            subtrees += [nucleus.all_trees() for nucleus in self.nuclei]
        if self.satellite is not None:
            subtrees.append(self.satellite.all_trees())
        subtrees = self.flatten(subtrees)
        if not self.is_leaf:
            subtrees.append(self)
        return subtrees

    def all_edus(self):
        """
        Retrieve a list of all EDUs from leafs of this tree.

        Returns:
            A list of EDUs in the form of lists of tokens for each EDU.
        """
        if self.text is not None:
            return [self.text]
        else:
            edus = []
            edus += [nucleus.all_edus() for nucleus in self.nuclei]
            if self.satellite is not None:
                edus += self.satellite.all_edus()
            return edus

def prepare_sentence(sentence):
    return re.findall(r"[\w']+|[\.,]+", sentence.lower(), re.UNICODE)

def merge_once(tree_list, get_vector, idx2relation, connection_model, relation_model, nuclearity_model):
    best_connection = 0
    best_connection_index = -1
    best_vector = None
    for i, (lhs, rhs) in enumerate(zip(tree_list, tree_list[1:])):
        vector = get_vector(lhs, rhs)
        if vector is None:
            continue
        connection = connection_model.predict(np.expand_dims(vector, axis=0))[0][0]
        if connection > best_connection:
            best_connection = connection
            best_connection_index = i
            best_vector = vector

    skip_prediction = False
    if best_vector is None:
        if len(tree_list) >= 2:
            best_connection_index = 0
            skip_prediction = True
        else:
            return tree_list, None

    lhs = tree_list[best_connection_index]
    rhs = tree_list[best_connection_index + 1]

    nuclei = []
    satellite = None

    if not skip_prediction:

        relation = relation_model.predict_classes(np.expand_dims(best_vector, axis=0))[0]
        relation = idx2relation[relation]
        nuclearity = nuclearity_model.predict(np.expand_dims(best_vector, axis=0))[0]
        nuclearity = np.round(nuclearity).astype(np.int)

        if nuclearity[0] == 0:
            satellite = lhs
        else:
            nuclei.append(lhs)
        if nuclearity[1] == 0:
            satellite = rhs
        else:
            nuclei.append(rhs)
    else:
        nuclei.append(lhs)
        satellite = rhs
        relation = "none"

    new_tree = RSTTree.from_trees(relation, nuclei, satellite)
    new_tree.construct_text()
    new_tree_list = tree_list[:best_connection_index]
    new_tree_list.append(new_tree)
    new_tree_list += tree_list[best_connection_index + 2:]
    return new_tree_list, new_tree

def merge(tree_list, get_vector, idx2relation, connection_model, relation_model, nuclearity_model):
    prev_length = len(tree_list)
    last_tree = None
    while True:
        tree_list, new_tree = merge_once(tree_list, get_vector, idx2relation, connection_model, relation_model, nuclearity_model)
        if new_tree is None or len(tree_list) == prev_length:
            return last_tree # return last successfully merged tree
        prev_length = len(tree_list)
        last_tree = new_tree

def create_tree(sentence_list, get_vector, idx2relation, connection_model, relation_model, nuclearity_model):
    tree_list = [RSTTree.from_text(prepare_sentence(sentence), i + 1, i + 1) for i, sentence in enumerate(sentence_list)]
    return merge(tree_list, get_vector, idx2relation, connection_model, relation_model, nuclearity_model)