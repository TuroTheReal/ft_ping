#******************************************************************************
# MAIN *
#******************************************************************************
NAME = ft_ping
NAME_BONUS = ft_ping_bonus

#******************************************************************************
# DIRECTORIES *
#******************************************************************************
OBJ_DEP_DIR = obj_n_dep/
OBJ_BONUS_DIR = obj_n_dep_bonus/

#******************************************************************************
# SOURCES, OBJECTS, HEADER & DEPENDENCIES *
#******************************************************************************
SRC_FOLD = src/
BONUS_FOLD = bonus/
HEADER_FOLD = header/
HEADER_SRC = ft_ping

C_SRC = main parse print socket resolve signal
# 		ping \
# 		icmp \
# 		network \
# 		stats \
# 		utils \
# 		errors

C_SRC_BONUS =

# Headers
HEADER_FLS = $(addsuffix .h, $(HEADER_SRC))
HEADER = $(addprefix $(HEADER_FOLD), $(HEADER_FLS))

# Mandatory
C_FLS = $(addsuffix .c, $(C_SRC))
SRC = $(addprefix $(SRC_FOLD), $(C_FLS))
OBJ = $(addprefix $(OBJ_DEP_DIR), $(SRC:.c=.o))
DEP = $(addprefix $(OBJ_DEP_DIR), $(OBJ:.o=.d))

# Bonus
C_FLS_BONUS = $(addsuffix .c, $(C_SRC_BONUS))
SRC_BONUS = $(addprefix $(BONUS_FOLD), $(C_FLS_BONUS))
OBJ_BONUS = $(addprefix $(OBJ_BONUS_DIR), $(SRC_BONUS:.c=.o))
DEP_BONUS = $(addprefix $(OBJ_BONUS_DIR), $(OBJ_BONUS:.o=.d))

OBJF = .cache_exists
OBJF_BONUS = .cache_exists_bonus

#******************************************************************************
# INSTRUCTIONS *
#******************************************************************************
CC = cc -g3
FLAGS = -Wall -Wextra -Werror -I$(HEADER_FOLD)
LDFLAGS = -lm
RM = rm -rf

#******************************************************************************
# COLORS *
#******************************************************************************
RESET = \033[0m
ROSE = \033[1;38;5;225m
VIOLET = \033[1;38;5;55m
VERT = \033[1;38;5;85m
BLEU = \033[1;34m

#******************************************************************************
# COMPILATION *
#******************************************************************************

all: $(NAME)
	@echo "$(ROSE)COMPILATION FINISHED, $(NAME) IS CREATED!$(RESET)"

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

bonus: $(NAME_BONUS)
	@echo "$(ROSE)COMPILATION FINISHED, $(NAME_BONUS) IS CREATED!$(RESET)"

$(NAME_BONUS): $(OBJ_BONUS)
	@$(CC) $(FLAGS) $(OBJ_BONUS) -o $(NAME_BONUS) $(LDFLAGS)

$(OBJ_DEP_DIR)%.o: %.c $(HEADER) | $(OBJF)
	@$(CC) $(FLAGS) -MMD -MP -c $< -o $@
	@echo "$(BLEU)Compiling $< to $@.$(RESET)"

$(OBJ_BONUS_DIR)%.o: %.c $(HEADER) | $(OBJF_BONUS)
	@$(CC) $(FLAGS) -MMD -MP -c $< -o $@
	@echo "$(BLEU)Compiling $< to $@.$(RESET)"

$(OBJF):
	@mkdir -p $(OBJ_DEP_DIR)$(SRC_FOLD)

$(OBJF_BONUS):
	@mkdir -p $(OBJ_BONUS_DIR)$(BONUS_FOLD)

clean:
	@$(RM) $(OBJ_DEP_DIR) $(OBJ_BONUS_DIR)
	@echo "$(VIOLET)Suppressing objects & dependencies files of $(NAME).$(RESET)"

fclean: clean
	@$(RM) $(NAME) $(NAME_BONUS)
	@echo "$(VERT)Suppressing archives $(NAME) and $(NAME_BONUS).$(RESET)"

re: fclean all

-include $(DEP)
-include $(DEP_BONUS)

.PHONY: re fclean clean all bonus