#lang racket/base

(require racket/string)
(require racket/file)
(require racket/list)

(define fst car)
(define snd cadr)
(define trd caddr)

;; ===========================================================================
;; Basic utility functions
;; ===========================================================================

(define (zip lists)
  ; zips lists together.
  ; places the car's in one list and the cadr's in another, etc...
  ; The length of the shortest inner list caps the length of the outputs
  ; ex:
  ;   (zip '((1 2 3) (a b c))) => '((1 a) (2 b) (3 c))
  ;   (zip '((1 2 'extra_item) (a b))) -> '((1 a) (2 b))
  (if (ormap null? lists)
      '()
      (cons (map car lists) (zip (map cdr lists)))))

(define (atom? x)
  ; #t if x is an atom in racket; #f otherwise.
  (and (not (null? x))
       (not (pair? x))))

(define (flatten lst)
  ; flattens a list to one level recursively. 
  ; resulting list only contains atoms.
  (if (atom? lst)
      (list lst)
      (apply append (map flatten lst))))

(define (find pred lst)
  ; find first matching occurence to pred in lst or return #f
  (cond
    ((null? lst)        #f)
    ((pred (car lst))   (car lst))
    (else               (find pred (cdr lst)))))

(define (max-by func lst)
  ; finds maximum value in lst using func for comparison.
  ;returns #f on empty lists
  (if (null? lst)
      #f
      (max-by-acc func (cdr lst) (func (car lst)) (car lst))))

(define (max-by-acc func lst threshold value)
  ; accumulator function used by max-by
  ; func, lst are the same as max-by
  ; threshold is the current maximum key
  ; value is the current value resulting in the maximum key.
  (define curr (if (null? lst) threshold (func (car lst))))
  (cond
    ((null? lst)           value)
    ((> curr threshold)    (max-by-acc func (cdr lst) curr (car lst)))
    (else                  (max-by-acc func (cdr lst) threshold value))))


;; ===========================================================================
;; Actual search done below.
;; ===========================================================================

(define edges
  (map (lambda (line)
         ; edge is read in the form "<vertex_from>\t<vertex_dest>\t<weight>"
         (define tokens (string-split line "\t" #:trim? #t))
         ; transform only third element (weight) to float/number
         (list (fst tokens) (snd tokens) (string->number (trd tokens))))
       (cdr (file->lines "./arbitrage.txt")))) ; trim off the '9'. not used.

(define vertices
  ; remove edge weights from edges and flatten. take unique values.
  (remove-duplicates
   (flatten
    ;removes each edge weight from each edge, leaving only vertices in list
    (map (lambda (edge) (list (fst edge) (snd edge)))
         edges))))


(define all-edges
  ; join all edges with their reciprocal weighted dual edges
  (append edges
          (map (lambda (edge)
                 ; a dual edge of (a b c) is (b a (/ 1 c))
                 ; (/ z) == (/ 1 z)
                 (list (snd edge) (fst edge) (/ (trd edge))))
               edges)))

(define (edge-pred search)
  ;create a predicate to filter/find edges by search
  (lambda (edge) (and (equal? (fst search) (fst edge))
                      (equal? (snd search) (snd edge)))))

(define (cost path)
  ; exchange arbitrage cost is the product of all edges
  (apply * (map (lambda (edge)
            ; get cost from each edge
            (trd (find (edge-pred edge) all-edges)))
          ; rotate list by one and zip with self to form edge pairs
          (zip (list path (append (cdr path) (list (car path)))))))) 

(define arbitrage
 (max-by cost 
         ; only tests combinations (drop outs) of at least length 2 (vertices)
         (filter (lambda (combo) (> (length combo) 1))
                 ; only tests combinations (drop outs) of best permutation
                 ; results in close to optimal answers in O(n!).
                 ; checking all possible permuted combos takes O(2^(n!))
                 (combinations (max-by cost (permutations vertices))))))

(display arbitrage)
(display " --> ")
(unless (equal? #f arbitrage) (display (cost arbitrage)))
(display "\n")