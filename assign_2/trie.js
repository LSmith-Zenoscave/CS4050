function Trie(words) {
    data = {}
    words.forEach(function (word) {
        var curr = self.data;
        var l = word.length;
        word.split('').forEach(function (c, idx) {
            var sub = curr[c] || [false, {}];
            if (idx == l - 1) {
                sub[0] = true;
            }
            curr[c] = sub;
            curr = sub[1];
        });
    });
    return data;
}

function findWithPrefix(prefix, trie) {
    var curr = trie;
    var isWord = false;
    var words = [];

    if (prefix.length > 0 && !Object.keys(trie).includes(prefix[0])) {
        return [];
    }

    prefix.split('').forEach(function (c) {
        var sub = curr[c] || [false, {}];
        isWord = sub[0];
        curr = sub[1];
    });

    if (isWord) {
        words.push(prefix);
    }
    
    Object.keys(curr).forEach(c => {
        findWithPrefix(c, curr).forEach(suffix => {
            words.push(prefix + suffix);
        });
    });

    return words;
}
