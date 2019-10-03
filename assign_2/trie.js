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

function findWithPrefix(prefix, trie, size) {
    var curr = trie;
    var isWord = false;
    var words = [];
    var count = 0;

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
        count++;
    }
    
    Object.keys(curr).forEach(c => {
        findWithPrefix(c, curr, size-count).forEach(suffix => {
            if (count < size)
            {
                words.push(prefix + suffix);
                count++;
            }
        });
    });

    return words.slice(0, size);
}
